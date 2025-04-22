<template>
	<div class="w-full max-w-full px-3 mb-6 sm:w-1/2 sm:flex-none xl:w-1/4">
		<div class="relative flex flex-col min-w-0 break-words bg-white shadow-soft-xl rounded-2xl bg-clip-border">
			<div class="flex-auto p-4">
				<WidgetSkeleton v-if="isLoading" :isLoading="isLoading" :rows="skeletonRows" :cols="skeletonCols"
					:withIcon="skeletonWithIcon" />
				<template v-else>
					<div class="flex flex-row -mx-3">
						<div class="flex-none w-2/3 max-w-full px-3">
							<div>
								<p class="mb-0 font-sans font-semibold leading-normal text-sm">
									{{ title }}
								</p>
								<h5 class="mb-0 font-bold">
									<slot name="body">
										{{ content }}
									</slot>
								</h5>
								<p v-if="subtext" class="text-xs text-gray-500">
									{{ subtext }}
								</p>
							</div>
						</div>
						<div class="flex justify-end px-3 basis-1/3">
							<div class="flex items-center justify-center w-12 h-12 text-center rounded-lg bg-gradient-to-tl"
								:class="iconBackground">
								<div class="flex items-center justify-center text-lg text-white">
									<i class="fas" :class="icon"></i>
								</div>
							</div>
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
