<template>
	<div id="widget" class="w-full max-w-full px-3 mb-6" :class="widgetWidth">
		<div
			class="relative h-full flex flex-col min-w-0 break-words bg-white shadow-soft-xl rounded-2xl bg-clip-border">
			<div class="h-full flex flex-col flex-auto p-4">
				<WidgetSkeleton v-if="isLoading" :isLoading="isLoading" :rows="skeletonRows" :cols="skeletonCols"
					:withIcon="skeletonWithIcon" />
				<template v-else>
					<!-- Header -->
					<div class="flex flex-none"
						:class="[`flex-${headerDirection}`, headerGap ? `gap-${headerGap}` : '', headerWrap ? 'flex-wrap' : 'flex-nowrap']">
						<slot name="header">
							<!-- Linker Bereich im Header -->
							<slot name="left">
								<div class="flex-none px-3" :class="leftWidth">
									<p class="mb-0 font-sans font-semibold leading-normal text-sm">
										<slot name="title">{{ title }}</slot>
									</p>
									<h5 class="mb-0 font-bold">
										<slot name="body">
											{{ content }}
										</slot>
									</h5>
									<template v-if="subtext">
										<div class="text-xs text-gray-500">
											<slot name="subtext">{{ subtext }}</slot>
										</div>
									</template>
								</div>
							</slot>

							<!-- Rechter Bereich im Header (Icon) -->
							<slot name="right">
								<div v-if="showIcon" class="flex justify-end px-3" :class="rightWidth">
									<div class="flex items-center justify-center w-12 h-12 text-center rounded-lg bg-gradient-to-tl cursor-pointer"
										:class="iconBackground" @click.stop="handleIconClick">
										<div class="flex items-center justify-center text-lg" :class="iconColor">
											<slot name="icon">
												<i v-if="icon" class="fas" :class="icon"></i>
											</slot>
										</div>
									</div>
								</div>
							</slot>
						</slot>
					</div>

					<!-- Content -->
					<div v-if="$slots.content"
						:class="['flex', 'flex-1', `flex-${contentDirection}`, contentWrap ? 'flex-wrap' : 'flex-nowrap', contentGap ? `gap-${contentGap}` : '', 'mt-4']">
						<div class="flex-1 px-3">
							<slot name="content" />
						</div>
					</div>
				</template>
			</div>
		</div>
	</div>
</template>

<script lang="ts">

export { default } from './widget'

</script>

<style src="./widget.scss"></style>
